/*
* Copyright (c) 2020 T-Mobile
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#ifndef __EFQDN_H
#define __EFQDN_H

#include "estring.h"
#include "eostring.h"

class EFqdn
{
public:
   EFqdn() {}
   EFqdn(cpStr val)
   {
      *this = val;
   }

   EFqdn &operator=(const EFqdn &val)
   {
      fqdn_ = val.fqdn_;
      return *this;
   }

   EFqdn &operator=(cpStr val)
   {
      cpStr ptr = val;
      UChar idx = 1;
      UChar label[256];
      fqdn_.clear();
      while (*ptr)
      {
         if (*ptr == '.')
         {
            label[0] = idx - 1;
            fqdn_.append(label, idx);
            idx = 1;
         }
         else
         {
            label[idx++] = *ptr;
         }
         ptr++;
      }

      if (idx > 0)
      {
         label[0] = idx - 1;
         fqdn_.append(label, idx);
      }

      return *this;
   }

   operator EOctetString() const
   {
      return fqdn_;
   }

   cpUChar data() const
   {
      return fqdn_.data();
   }

   size_t length() const
   {
      return fqdn_.length();
   }

   EString toString() const
   {
      EString str;
      Char label[256];
      pChar lblptr = label;

      for (EOctetString::size_type i = 0; i < fqdn_.size(); )
      {
         UChar len = fqdn_[i++];
         UChar j = 0;
         if (!str.empty())
            label[j++] = '.';
         lblptr = &label[j];
         while (len--)
         {
            *lblptr = static_cast<Char>(fqdn_[i++]);
            lblptr++;
         }
         str.append(label, lblptr - label);
      }

      return str;
   }
   
private:
   EOctetString fqdn_;
};

#endif // #ifndef __EFQDN_H