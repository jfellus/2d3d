/**
 * \file NativePointer.java
 * \author Romain Negrel
 * \version 4.0
 */

package retin.toolbox.document.basic;

import retin.toolbox.document.Document;

public class NativePointer extends Document
{
    private long ptr;
    
    public NativePointer(){
        ptr = 0;
    }
    
    public void set(long ptr){
        this.ptr = ptr;
    }
    
    public long get() {
        return this.ptr;
    }
    
    public boolean isNull(){
        return ptr==0;
    }
                
}