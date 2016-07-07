import QtQuick 2.4

import Manipulation 1.0

ListModel {
    id : myModel

    function checkIfNameExist(searchName)
    {
        for (var i = 0; i < myModel.count; i++)
        {
            if(myModel.get(i).name === searchName)
            {
                return true
            }
        }
        return false
    }

    function getElementByType(type)
    {
        // use reverse order to get correct Neuroma Pad data
        for (var i = myModel.count - 1; i >= 0; --i)
        {
            if (myModel.get(i).type === type)
            {
                return myModel.get(i)
            }
        }
        return undefined
    }

    ListElement {
        svgItem : "qrc:/exampleImages/3.svg"
        name : "U-Shaped Pad"
        type : Manipulation.U_Pad
        m_Width : -1
        m_Height : -1
        basicRotation : 0
    }
    ListElement {
        name : "Create Custom Pad"
        type : Manipulation.KCustom
        m_Width : -1
        m_Height : -1
        basicRotation : 0
    }
    ListElement {
        svgItem : "qrc:/exampleImages/4.svg"
        name : "Heel Pad"
        type : Manipulation.Heal_pad
        m_Width : -1
        m_Height : -1
        basicRotation : 0
    }
    ListElement {
        svgItem : "qrc:/exampleImages/5.svg"
        name : "Heel Spur Hole"
        type : Manipulation.Heal_spur_hole
        m_Width : -1
        m_Height : -1
        basicRotation : 0
    }
    ListElement {
        svgItem : "qrc:/exampleImages/6.svg"
        name : "Met Pad"
        type : Manipulation.Met_Pad
        m_Width : -1
        m_Height : -1
        basicRotation : 0
    }
    ListElement {
        svgItem : "qrc:/exampleImages/7.svg"
        name : "Met Bar Pad"
        type : Manipulation.Met_Bar_Pad
        m_Width : -1
        m_Height : -1
        basicRotation : 0
    }
    ListElement {
        svgItem : "qrc:/exampleImages/8.svg"
        name : "Dancer's Pad"
        type : Manipulation.Dancer_Pad
        m_Width : -1
        m_Height : -1
        basicRotation : 0
    }
    ListElement {
        svgItem : "qrc:/exampleImages/9.svg"
        name : "Morton's Extension"
        type : Manipulation.Mortons_Extension
        m_Width : -1
        m_Height : -1
        basicRotation : 0
    }

    ListElement {
        svgItem : "qrc:/exampleImages/10.svg"
        name : "Neuroma Pad"
        type : Manipulation.Mortons_Neroma_Pad
        m_Width : -1
        m_Height : -1
        basicRotation : 0
    }
    ListElement {
        svgItem : "qrc:/exampleImages/11.svg"
        name : "Scaphoid Pad"
        type : Manipulation.Scaphoid_Pad
        m_Width : -1
        m_Height : -1
        basicRotation : 0
    }
}
