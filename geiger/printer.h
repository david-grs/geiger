#pragma once

#include <string>

namespace geiger
{

struct suite_base;
struct suite_report;
struct test_report;

struct printer_base
{
    virtual ~printer_base() {}

    virtual void on_start(const suite_base&) {}
    virtual void on_test_complete(const std::string&, const test_report&) {}
    virtual void on_complete(const suite_report&) {}
};

}
