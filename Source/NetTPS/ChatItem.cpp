#include "ChatItem.h"
#include "Components/TextBlock.h"

void UChatItem::SetContent(FText s)
{
    content->SetText(s);
}
