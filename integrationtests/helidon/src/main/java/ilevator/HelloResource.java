package ilevator;

import javax.ws.rs.GET;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.QueryParam;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;

import org.apache.commons.lang3.StringUtils;

@Path("hello")
public class HelloResource {

	@GET
	@Produces(MediaType.TEXT_PLAIN)
	public Response sayHello(@QueryParam("q") String name) {
		return Response.ok("Hello " + StringUtils.defaultString(name, "World")).header("ILEVATOR_HELLO_QUERY", name)
				.build();
	}
	
}
