package ilevator;

import javax.ws.rs.GET;
import javax.ws.rs.Path;
import javax.ws.rs.PathParam;
import javax.ws.rs.core.Context;
import javax.ws.rs.core.HttpHeaders;

@Path("headers")
public class HeaderResource {
	
	@Path("{id}")
	@GET
	public String getHeaderValue(@Context HttpHeaders headers, @PathParam("id") String id) {
		return headers.getHeaderString(id);
	}
}
