//------------------------------------------------------------------------------------------------
//! JSON helper struct for storing vectors in an array
class ACE_VectorStruct : SCR_JsonApiStruct {
    // short name for more efficient serialization
    protected vector v;

    //------------------------------------------------------------------------------------------------
    void ACE_VectorStruct() {
        RegV("v");
    }

    //------------------------------------------------------------------------------------------------
    // Print out contents of saved data.
    override void Log() {
        Print(v.ToString());
    }

    //------------------------------------------------------------------------------------------------
    void SetVector(vector vec) {
        v = vec;
    }

    //------------------------------------------------------------------------------------------------
    vector GetVector() {
        return v;
    }
}
