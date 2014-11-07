package retin.toolbox.document.buffer;

/**
 * Représente le pointeur d'un buffer bas niveau.
 */
public final class NativeFloatBuffer {
   
    private long length;
    private long bufferPtr;
    
    public NativeFloatBuffer() {
        bufferPtr = 0;
        length = 0;
    }

    public NativeFloatBuffer(long size) {
        resize(size,false);
    }
    
    @Override
    public void finalize() {
        release();        
    }
    
    public native void release();
    
    public long size() { return length; }
    
    /**
     * Modifie la taille du buffer.
     * @param size Nouvelle taille.
     * @param bKeepData Indique s'il faut conserver les anviennes valeurs.
     */
    public native void resize (long size,boolean bKeepData);
    
    /**
     * Définie les valeurs du buffer sur [i,i+values.length[
     * @param values Valeurs à utiliseurs
     */
    public native void setValues (long i,float[] values);
    
    /**
     * Modifie la taille du buffer et recopie les valeurs de [seek,seek+size[
     * @param values Valeurs à utiliseurs
     */
    public native void cpy(float[] values, long seek, long size);
}
