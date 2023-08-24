package ilevator;

import javax.ws.rs.Consumes;
import javax.ws.rs.POST;
import javax.ws.rs.Path;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;

import org.glassfish.jersey.media.multipart.FormDataBodyPart;
import org.glassfish.jersey.media.multipart.FormDataParam;

@Path("multipart")
public class MultiPartResource {

	@POST
	@Path("form")
	@Consumes(MediaType.MULTIPART_FORM_DATA)
	public Response postFormDataTextOnly(@FormDataParam("user") FormDataBodyPart username,
			@FormDataParam("email") FormDataBodyPart email) {
		return Response.ok(username.getValue() + " - " + email.getValue()).build();
	}

}
