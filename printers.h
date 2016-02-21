#pragma once

#include <string>

namespace disco
{

struct suite_base;
struct suite_report;
struct test_report;

struct printer_base
{
    virtual ~printer_base()
    {
    }

    virtual void on_start(const suite_base& s)
    {
    }
    virtual void on_test_complete(const std::string& name, const test_report& r)
    {
    }
    virtual void on_complete(const suite_report& r)
    {
    }
};

namespace printers
{

struct console : public printer_base
{
    void on_start(const suite_base& s) override;
    void on_test_complete(const std::string& name, const test_report& r) override;

   private:
    int m_first_col_width;
};
}
}
