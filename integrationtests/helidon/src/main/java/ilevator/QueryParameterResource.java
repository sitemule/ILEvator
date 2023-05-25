package ilevator;

import javax.ws.rs.GET;
import javax.ws.rs.Path;
import javax.ws.rs.PathParam;
import javax.ws.rs.core.Context;
import javax.ws.rs.core.UriInfo;

@Path("parameters")
public class QueryParameterResource {

	@Path("{id}")
	@GET
	public String getQueryParameterValue(@Context UriInfo info, @PathParam("id") String id) {
		return info.getQueryParameters().getFirst(id);
	}
}
