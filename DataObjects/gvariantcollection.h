/*Copyright 2010 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GVARIANTCOLLECTION_H
#define GVARIANTCOLLECTION_H

#include "gutil_macros.h"
#include "Custom/gvariant.h"
#include "Custom/observablegvariant.h"
#include "DataObjects/collection.h"

GUTIL_BEGIN_NAMESPACE( DataObjects );


// A collection of QVariants, built on top of our Collection class
typedef ResizableCollection<Custom::GVariant> GVariantCollection;


template <class ObservingClass> class ObservableGVariantCollection :
        public ResizableCollection< Custom::ObservableGVariant<ObservingClass> >
{
public:
    inline ObservableGVariantCollection(int size = 0)
        :ResizableCollection< Custom::ObservableGVariant<ObservingClass> >(size),
        _reference(0)
    {
        _value_about_to_change_func = 0;
        _value_changed_func = 0;
    }

    inline ObservableGVariantCollection(
            const ResizableCollection< Custom::ObservableGVariant<ObservingClass> > &v)
        : ResizableCollection< Custom::ObservableGVariant<ObservingClass> >(v),
        _reference(0)
    {
        _value_about_to_change_func = 0;
        _value_changed_func = 0;
    }


    inline void SetValueAboutToChangeFunction(
            ObservingClass *instance,
            void (ObservingClass::*func)(const Custom::GVariant &current, const Custom::GVariant &future)){
        _reference = instance;
        _value_about_to_change_func = func;

        for(int i = 0;
            i < ResizableCollection< Custom::ObservableGVariant<ObservingClass> >::Count();
            i++)
        {
            ResizableCollection< Custom::ObservableGVariant<ObservingClass> >::
                    At(i).SetValueAboutToChangeFunction(_reference, _value_changed_func);
        }
    }

    inline void SetValueChangedFunction(ObservingClass *instance,
            void (ObservingClass::*func)(const Custom::GVariant &oldval, const Custom::GVariant &newval)){
        _reference = instance;
        _value_changed_func = func;

        for(int i = 0;
            i < ResizableCollection< Custom::ObservableGVariant<ObservingClass> >::Count();
            i++)
        {
            ResizableCollection< Custom::ObservableGVariant<ObservingClass> >::
                    At(i).SetValueChangedFunction(_reference, _value_changed_func);
        }
    }


protected:

    void on_add(Custom::ObservableGVariant<ObservingClass> *ogv){
        if(_reference)
        {
            ogv->SetValueAboutToChangeFunction(_reference, _value_about_to_change_func);
            ogv->SetValueChangedFunction(_reference, _value_changed_func);
        }
    }


private:

    ObservingClass *_reference;
    void (ObservingClass::*_value_about_to_change_func)(const Custom::GVariant &, const Custom::GVariant &);
    void (ObservingClass::*_value_changed_func)(const Custom::GVariant &, const Custom::GVariant &);

};

GUTIL_END_NAMESPACE

#endif // GVARIANTCOLLECTION_H
