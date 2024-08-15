package ilevator;

import jakarta.ws.rs.GET;
import jakarta.ws.rs.Path;
import jakarta.ws.rs.PathParam;
import jakarta.ws.rs.core.Context;
import jakarta.ws.rs.core.UriInfo;

@Path("parameters")
public class QueryParameterResource {

	@Path("{id}")
	@GET
	public String getQueryParameterValue(@Context UriInfo info, @PathParam("id") String id) {
		return info.getQueryParameters().getFirst(id);
	}
}
