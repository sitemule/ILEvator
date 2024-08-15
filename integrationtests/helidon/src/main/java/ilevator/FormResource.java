package ilevator;

import jakarta.ws.rs.Consumes;
import jakarta.ws.rs.FormParam;
import jakarta.ws.rs.POST;
import jakarta.ws.rs.Path;
import jakarta.ws.rs.Produces;
import jakarta.ws.rs.core.MediaType;
import jakarta.ws.rs.core.Response;

@Path("form")
public class FormResource {

	@POST
	@Consumes(MediaType.APPLICATION_FORM_URLENCODED)
	@Produces(MediaType.TEXT_PLAIN)
	public Response register(@FormParam("firstName") String firstName, @FormParam("lastName") String lastName) {
		if (firstName == null || lastName == null)
			return Response.status(400).build();
		else
			return Response.ok(firstName + " " + lastName).build();
	}
}
