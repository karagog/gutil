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

#include "expander.h"
#include "gutil_macros.h"
#include <QShowEvent>
GUTIL_USING_NAMESPACE(Controls);

Expander::Expander(QWidget *parent) :
    QWidget(parent),
    m_height(-1)
{
    setVisible(false);
}

void Expander::SetExpanded(bool dir)
{
    //m_height = height();
    setVisible(dir);
    emit Expanded(dir);
}

void Expander::hideEvent(QHideEvent *ev)
{
    //parentWidget()->adjustSize();
//    if(m_height != -1)
//    {
//        parentWidget()->resize(parentWidget()->width(),
//                               parentWidget()->height() - m_height);
//        m_height = -1;
//    }
}
