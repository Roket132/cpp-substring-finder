#ifndef PROGRESS_BAR_STYLE_H
#define PROGRESS_BAR_STYLE_H

#include <string>
#include <QString>

class progress_bar_style {
public:
    progress_bar_style();

    QString default_style;

    QString run_style;

    QString stop_style;
};

#endif // PROGRESS_BAR_STYLE_H
