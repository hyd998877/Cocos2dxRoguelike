/**

@file EditTextDialogLayout.h

@brief description

Cocos2dRogueLike

@author kyokomi
@date 2014/06/28

*/

#ifndef __Cocos2dRogueLike__EditTextDialogLayout__
#define __Cocos2dRogueLike__EditTextDialogLayout__

#include "cocos2d.h"

#include "ModalLayer.h"

/**
@class EditTextDialogLayout EditTextDialogLayout.h

@brief description

detail description

@author kyokomi
@date 2014/06/28
*/
class EditTextDialogLayout : public ModalLayer
{
    
public:
    typedef std::function<void(std::string inputText)> EditTextDialogCallback;
    
    EditTextDialogLayout();
    virtual ~EditTextDialogLayout();
    virtual bool init();
    CREATE_FUNC(EditTextDialogLayout);
    
    static EditTextDialogLayout* create(const EditTextDialogCallback& callback);

    void setExecuteButtonText(const std::string& buttonText);
protected:
    
private:
    cocos2d::Node* initLayout();
    
    cocos2d::Node* _editDialogLayout;
    EditTextDialogCallback _callback;
};

#endif /* defined(__Cocos2dRogueLike__EditTextDialogLayout__) */
