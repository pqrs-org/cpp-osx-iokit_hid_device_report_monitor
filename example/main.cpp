#include <IOKit/hid/IOHIDElement.h>
#include <IOKit/hid/IOHIDUsageTables.h>
#include <IOKit/hid/IOHIDValue.h>
#include <csignal>
#include <pqrs/osx/iokit_hid_device_report_monitor.hpp>
#include <pqrs/osx/iokit_hid_manager.hpp>

namespace {
auto global_wait = pqrs::make_thread_wait();
}

int main(void) {
  std::signal(SIGINT, [](int) {
    global_wait->notify();
  });

  auto time_source = std::make_shared<pqrs::dispatcher::hardware_time_source>();
  auto dispatcher = std::make_shared<pqrs::dispatcher::dispatcher>(time_source);
  auto run_loop_thread = std::make_shared<pqrs::cf::run_loop_thread>();

  std::unordered_map<pqrs::osx::iokit_registry_entry_id::value_t, std::shared_ptr<pqrs::osx::iokit_hid_device_report_monitor>> monitors;

  std::vector<pqrs::cf::cf_ptr<CFDictionaryRef>> matching_dictionaries{
      pqrs::osx::iokit_hid_manager::make_matching_dictionary(
          pqrs::hid::usage_page::generic_desktop,
          pqrs::hid::usage::generic_desktop::keyboard),

      pqrs::osx::iokit_hid_manager::make_matching_dictionary(
          pqrs::hid::usage_page::generic_desktop,
          pqrs::hid::usage::generic_desktop::mouse),

      pqrs::osx::iokit_hid_manager::make_matching_dictionary(
          pqrs::hid::usage_page::generic_desktop,
          pqrs::hid::usage::generic_desktop::pointer),
  };

  auto hid_manager = std::make_unique<pqrs::osx::iokit_hid_manager>(dispatcher,
                                                                    run_loop_thread,
                                                                    matching_dictionaries);

  hid_manager->device_matched.connect([dispatcher, run_loop_thread, &monitors](auto&& registry_entry_id, auto&& device_ptr) {
    if (device_ptr) {
      auto hid_device = pqrs::osx::iokit_hid_device(*device_ptr);
      std::cout << "device_matched registry_entry_id:" << registry_entry_id << std::endl;
      if (auto manufacturer = hid_device.find_string_property(CFSTR(kIOHIDManufacturerKey))) {
        std::cout << "  manufacturer:" << *manufacturer << std::endl;
      }
      if (auto product = hid_device.find_string_property(CFSTR(kIOHIDProductKey))) {
        std::cout << "  product:" << *product << std::endl;
      }
      if (auto vendor_id = hid_device.find_int64_property(CFSTR(kIOHIDVendorIDKey))) {
        std::cout << "  vendor_id:" << *vendor_id << std::endl;
      }
      if (auto product_id = hid_device.find_int64_property(CFSTR(kIOHIDProductIDKey))) {
        std::cout << "  product_id:" << *product_id << std::endl;
      }

      auto m = std::make_shared<pqrs::osx::iokit_hid_device_report_monitor>(dispatcher,
                                                                            run_loop_thread,
                                                                            *device_ptr);
      monitors[registry_entry_id] = m;

      m->started.connect([registry_entry_id] {
        std::cout << "started " << registry_entry_id << std::endl;
      });

      m->stopped.connect([registry_entry_id] {
        std::cout << "stopped " << registry_entry_id << std::endl;
      });

      m->report_arrived.connect([](auto&& type, auto&& report_id, auto&& report_buffer) {
        std::cout << "type:" << type
                  << ", report_id:" << report_id
                  << std::endl;
      });

      m->error_occurred.connect([](auto&& message, auto&& iokit_return) {
        std::cerr << "error_occurred " << message << " " << iokit_return << std::endl;
      });

      m->async_start(kIOHIDOptionsTypeNone,
                     std::chrono::milliseconds(3000));
    }
  });

  hid_manager->device_terminated.connect([&monitors](auto&& registry_entry_id) {
    std::cout << "device_terminated registry_entry_id:" << registry_entry_id << std::endl;
    monitors.erase(registry_entry_id);
  });

  hid_manager->error_occurred.connect([](auto&& message, auto&& iokit_return) {
    std::cerr << "error_occurred " << message << " " << iokit_return << std::endl;
  });

  hid_manager->async_start();

  // ============================================================

  global_wait->wait_notice();

  // ============================================================

  hid_manager = nullptr;
  monitors.clear();

  run_loop_thread->terminate();
  run_loop_thread = nullptr;

  dispatcher->terminate();
  dispatcher = nullptr;

  std::cout << "finished" << std::endl;

  return 0;
}
