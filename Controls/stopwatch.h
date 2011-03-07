/*Copyright 2011 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <QWidget>
#include "Utils/stopwatchengine.h"

class QLabel;
class QPushButton;

namespace GUtil{ namespace Controls{


class Stopwatch :
        public QWidget
{
    Q_OBJECT
public:

    explicit Stopwatch(QWidget *parent = 0);

    enum ResolutionEnum
    {
        Milliseconds,
        HundredMilliseconds,
        Seconds
    };

    void SetResolution(ResolutionEnum);

    inline bool IsRunning() const{
        return _stopwatch.IsRunning();
    }
    inline QDateTime TimeStart() const{
        return _stopwatch.TimeStart();
    }
    inline QDateTime TimeEnd() const{
        return _stopwatch.TimeEnd();
    }
    inline QDateTime TimeStop() const{
        return _stopwatch.TimeStop();
    }


public slots:

    void Start();
    void Stop();
    void StartStop(bool start);

    void Reset();


private slots:

    void _update_label();


private:

    QLabel *lbl_startTime;
    QLabel *lbl_endTime;
    QLabel *lbl_time;
    QPushButton *btn_start, *btn_reset;

    Utils::StopwatchEngine _stopwatch;

    ResolutionEnum m_resolution;

};


}}

#endif // STOPWATCH_H
