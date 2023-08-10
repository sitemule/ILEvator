package ilevator;

import javax.ws.rs.Consumes;
import javax.ws.rs.FormParam;
import javax.ws.rs.POST;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;

@Path("form")
public class FormResource {

	@POST
	@Consumes(MediaType.APPLICATION_FORM_URLENCODED)
	@Produces(MediaType.TEXT_PLAIN)
	public Response register(@FormParam("firstName") String firstName, @FormParam("lastName") String lastName) {
		if (firstName == null || lastName == null) return Response.status(400).build();
		else return Response.ok(firstName + " " + lastName).build();
	}
}
