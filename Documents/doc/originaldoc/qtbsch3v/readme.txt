                Qt-BSch3V Version 0.47
                     2006/07/08

1. About Qt-BSch3V
-------------------

Qt-BSch3V(Qt-Basic Schematic 3V) is a schematic capture based on Qt.
It has compatibility with BSch3V(Suigyodo's schematic capture for Windows), 
except Qt-BSch3V doesn't support layer.i

It needs Qt 4.1.2, please refer to Trolltech(http://www.trolltech.com) 
about Qt.

2. Files
----------

  readme.txt       This file
  readme-j.txt     Japanese readme
  qtbsch3v/        Source file
  LIBV/*.lb3       Component library
  COPYING          License
  
In the source file, the comment is written in Japanese language using 
character code EUC-JP. 


3. Tested environment
--------------------------

I tested compiling and operation in the following environment;
 Windows XP pro SP2 + Qt 4.1.2 opensource edition
 Knoppix Linux 3.8.1 on HDD + Qt 4.1.2 opensource edition


4. How to compile
------------------

From command prompt, enter followings;

-----------------------------------
$cd qtbsch3v
$qmake
$make
-----------------------------------

qtbsch3v.pro exists in qtbsch3v/.


5. Setup
---------

It's necessary to set up libraries and a font before drawing schematics with 
Qt-BSch3V.

5-1. Default Comment Font
It is used when you place a comment text.
To set up default font, click [default Comment font] on the [Set]  menu.

5-2. Label Font
It is used for displaying labels, tags, component's pin-names...
To set up label font, click [Label Font] on the [Set]  menu.

5-3. Library
(1)On [Set] menu, click [Library].
(2)Click [Add] button and Open dialog will appear.
(3)Select libraries and click [OK].
(4)To change the priority of a library, select the library and click [Up]/
   [Down]button.
(5)Click [OK].


6. Operation hints
-------------------

6-1. Selection
Qt-BSch3V has two kinds of selection tools, "Selector-tool" and "Drag-tool".

(1)In order to change the attribute of an comment, tag, label, and comment,
   use the "Selector-tool", and select it singly. 

(2)To move, cut, copy the objects, use "Selector-tool".

(3)To drag objects with keeping connection, use "Drag-tool".

6-2. Wire and Junction
(1)To make a slanting wire, use "Drag-tool".

(2)Place junction
  - Select the "Junction-tool", and click at where to place the junction.
  - Select the "Wire-tool", and double click at where to place the junction.

6-3. Wire-entry and Bus-entry
(1)Place entry
  Select the "*-entry-tool", and click at where to place entry.

(2)To change the direction of Wire-entry and Bus-entry, click the [Rotate] or
  [Mirror] on the [Edit] menu.

6-4. Component
(1)Place component
  Select the "Component-tool", click at where to place the component, and you 
  will find dialog for choosing component.
  If you find no component in the dialog, please check the library setting.

(2)To change the name and/or the reference number of components, 
   1)Select the component singly using the "Selector-tool". 
   2)Open the [Edit] menu, and select the [Attribute].

(3)To move the name and/or the reference number of components, 
   1)Select the component singly using the "Selector-tool".
   2)Drag the name and/or the reference number.

6-5. Tag, Label, Comment
(1)Place these objects
  Select select the corresponding tool, and click at where to place the object.
  Then the attribute dialog will appear.

(2)To change the attribute of these objects
   1)Select the object singly using the "Selector-tool". 
   2)Open the [Edit] menu, and select the [Attribute].

6-6. Anchor of Labels
If you use the Netlist software(external command), the anchor point of the label
is very important.
In the Netlist software, a label gives signal name to the wire which is on its
anchor point.

(1)Horizontal label
  The anchor is at the lower left of the label.
  Horizontal label gives the signal name to only horizontal wire.

(2)Vertical label
  The anchor is at the lower right of the label.
  Vertical label gives the signal name to only vertical wire.


6-7. Library file
To create an original library, you can use Qt-LCoV.


7. License
----------

This program is a free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


--------------------------------------------------------------------------------

  OKADA, Hitoshi
  URL    : http://www.suigyodo.com/online/e
  E-Mail : hitoshi@suigyodo.com
