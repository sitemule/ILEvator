package ilevator;

import org.apache.commons.lang3.StringUtils;

import jakarta.ws.rs.GET;
import jakarta.ws.rs.Path;
import jakarta.ws.rs.Produces;
import jakarta.ws.rs.QueryParam;
import jakarta.ws.rs.core.MediaType;
import jakarta.ws.rs.core.Response;

@Path("hello")
public class HelloResource {

	@GET
	@Produces(MediaType.TEXT_PLAIN)
	public Response sayHello(@QueryParam("q") String name) {
		return Response.ok("Hello " + StringUtils.defaultString(name, "World")).header("ILEVATOR_HELLO_QUERY", name)
				.build();
	}

}
