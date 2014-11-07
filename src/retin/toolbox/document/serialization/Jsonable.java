package retin.toolbox.document.serialization;

import java.io.IOException;

public interface Jsonable {

    public void    toJsonValue(JsonWriter out) throws IOException;

}
