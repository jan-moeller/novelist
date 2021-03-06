/**********************************************************
 * @file   SettingsPage_Stats.cpp
 * @author jan
 * @date   11/19/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include "SettingsPage_Stats.h"
#include "ui_SettingsPage_Stats.h"
#include "ProjectStatCollector.h"

namespace novelist {
    SettingsPage_Stats::SettingsPage_Stats(QWidget* parent, Qt::WindowFlags f)
            :QWidget(parent, f),
             m_ui{std::make_unique<Ui::SettingsPage_Stats>()}
    {
        m_ui->setupUi(this);
    }

    SettingsPage_Stats::~SettingsPage_Stats() = default;

    void SettingsPage_Stats::retranslateUi()
    {
        m_ui->retranslateUi(this);
    }

    void SettingsPage_Stats::changeEvent(QEvent* event)
    {
        QWidget::changeEvent(event);
        switch (event->type()) {
            case QEvent::LanguageChange:
                retranslateUi();
                break;
            default:
                break;
        }
    }

    SettingsPage_Stats_Creator::SettingsPage_Stats_Creator(gsl::not_null<ProjectStatCollector*> collector) noexcept
            :m_collector(collector)
    {
    }

    SettingsPage_Stats_Creator::~SettingsPage_Stats_Creator() = default;

    QString SettingsPage_Stats_Creator::name() noexcept
    {
        return tr("Statistics");
    }

    QString SettingsPage_Stats_Creator::uid() noexcept
    {
        return "stats";
    }

    void SettingsPage_Stats_Creator::initialize(QWidget* widget, QSettings const& settings) noexcept
    {
        restoreDefaults(widget);

        auto* page = dynamic_cast<SettingsPage_Stats*>(widget);
        if (settings.contains("interval"))
            page->m_ui->spinBoxTiming->setValue(settings.value("interval").toInt());
    }

    void SettingsPage_Stats_Creator::apply(QWidget const* widget, QSettings& settings) noexcept
    {
        auto* page = dynamic_cast<SettingsPage_Stats const*>(widget);

        settings.setValue("interval", page->m_ui->spinBoxTiming->value());
    }

    void SettingsPage_Stats_Creator::initiateUpdate(QSettings const& settings) noexcept
    {
        m_collector->setWatchInterval(settings.value("interval").toInt());
    }

    void SettingsPage_Stats_Creator::restoreDefaults(QWidget const* widget) noexcept
    {
        auto* page = dynamic_cast<SettingsPage_Stats const*>(widget);

        page->m_ui->spinBoxTiming->setValue(15);
    }

    QWidget* SettingsPage_Stats_Creator::createWidget() noexcept
    {
        return new SettingsPage_Stats;
    }
}