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

#ifndef EXPANDER_H
#define EXPANDER_H

#include <QWidget>

namespace GUtil{ namespace Controls{


class Expander :
        public QWidget
{
    Q_OBJECT
public:

    explicit Expander(QWidget *parent = 0);

    inline bool IsExpanded() const{
        return isVisible();
    }


signals:

    void Expanded(bool);


public slots:

    void SetExpanded(bool);


protected:

    virtual void hideEvent(QHideEvent *);


private:

    int m_height;

};


}}

#endif // EXPANDER_H
