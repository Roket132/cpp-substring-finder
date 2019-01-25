#include "progress_bar_style.h"


progress_bar_style::progress_bar_style() :
    default_style("QProgressBar {"
                  "border: 2px solid grey;"
                  "border-radius: 5px;"
                  "text-align: center;"
              "}"
              "QProgressBar::chunk {"
                  "background-color: rgb(84, 84, 84);"
                  "width: 10px;"
                  "margin: 0.5px;"
              "}") ,
    run_style("QProgressBar {"
              "border: 2px solid grey;"
              "border-radius: 5px;"
              "text-align: center;"
          "}"
          "QProgressBar::chunk {"
              "background-color: rgb(88, 245, 49);"
              "width: 10px;"
              "margin: 0.5px;"
          "}") ,
    stop_style("QProgressBar {"
               "border: 2px solid grey;"
               "border-radius: 5px;"
               "text-align: center;"
           "}"
           "QProgressBar::chunk {"
               "background-color: rgb(244, 0, 0);"
               "width: 10px;"
               "margin: 0.5px;"
               "}")
{

}
