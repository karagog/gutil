/*Copyright 2010-2013 George Karagoulis

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

#ifndef GUTIL_NO_GUI_FUNCTIONALITY

#include <gutil/stopwatchengine.h>
#include <QPushButton>

class QLabel;
class QPushButton;

namespace GUtil{ namespace Qt{


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

    bool IsRunning(){
        return _stopwatch.IsRunning();
    }
    QDateTime TimeStart(){
        return _stopwatch.TimeStart();
    }
    QDateTime TimeCurrent(){
        return _stopwatch.TimeCurrent();
    }
    QDateTime TimeStopped(){
        return _stopwatch.TimeStopped();
    }


public slots:

    /** Activates the stopwatch (starts timing) */
    void Start(){ StartStop(true); }

    /** Deactivates the stopwatch (stops timing) */
    void Stop(){ StartStop(false); }

    /** Activates/Deactivates the stopwatch, depending on the argument */
    void StartStop(bool start){
        _stopwatch.StartStop(start);
        _update_label();
    }

    /** Resets the stopwatch back to 0 */
    void Reset(){
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

    StopwatchEngine _stopwatch;

    ResolutionEnum m_resolution;

};


}}

#endif // GUI_FUNCTIONALITY

#endif // STOPWATCH_H
