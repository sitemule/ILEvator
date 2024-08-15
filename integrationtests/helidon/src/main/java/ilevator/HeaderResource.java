package ilevator;

import jakarta.ws.rs.GET;
import jakarta.ws.rs.Path;
import jakarta.ws.rs.PathParam;
import jakarta.ws.rs.core.Context;
import jakarta.ws.rs.core.HttpHeaders;

@Path("headers")
public class HeaderResource {

	@Path("{id}")
	@GET
	public String getHeaderValue(@Context HttpHeaders headers, @PathParam("id") String id) {
		return headers.getHeaderString(id);
	}
}
