#include "TransferFunctionEditor.h"
#include <kvs/RGBColor>


namespace kvs
{

namespace glut
{

TransferFunctionEditor::TransferFunctionEditor( kvs::ScreenBase* parent ):
    kvs::TransferFunctionEditorBase( parent )
{
    const kvs::RGBColor base_color( 50, 50, 50 );
    this->setBackgroundColor( base_color );
    this->setup( this );
}

} // end of namespace glut

} // end of namespace kvs
