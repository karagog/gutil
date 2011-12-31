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

#ifdef GUI_FUNCTIONALITY

#include "Utils/stopwatchengine.h"
#include <QPushButton>

class QLabel;
class QPushButton;

namespace GUtil{ namespace QT{ namespace Controls{


/** Implements a simple stopwatch widget. */
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

    inline bool IsRunning(){
        return _stopwatch.IsRunning();
    }
    inline QDateTime TimeStart(){
        return _stopwatch.TimeStart();
    }
    inline QDateTime TimeCurrent(){
        return _stopwatch.TimeCurrent();
    }
    inline QDateTime TimeStopped(){
        return _stopwatch.TimeStopped();
    }


public slots:

    /** Activates the stopwatch (starts timing) */
    inline void Start(){ StartStop(true); }

    /** Deactivates the stopwatch (stops timing) */
    inline void Stop(){ StartStop(false); }

    /** Activates/Deactivates the stopwatch, depending on the argument */
    void StartStop(bool start){
        _stopwatch.StartStop(start);
        _update_label();
    }

    /** Resets the stopwatch back to 0 */
    inline void Reset(){
        if(btn_start->isChecked()) btn_start->setChecked(false);
        _stopwatch.Reset();
        _update_label();
    }


private slots:

    void _update_label(const QDateTime &t = QDateTime());


private:

    QLabel *lbl_startTime;
    QLabel *lbl_endTime;
    QLabel *lbl_time;
    QPushButton *btn_start, *btn_reset;

    Utils::StopwatchEngine _stopwatch;

    ResolutionEnum m_resolution;

};


}}}

#endif // GUI_FUNCTIONALITY

#endif // STOPWATCH_H
