package ilevator;

import jakarta.ws.rs.DELETE;
import jakarta.ws.rs.GET;
import jakarta.ws.rs.HEAD;
import jakarta.ws.rs.OPTIONS;
import jakarta.ws.rs.PATCH;
import jakarta.ws.rs.PUT;
import jakarta.ws.rs.Path;
import jakarta.ws.rs.Produces;
import jakarta.ws.rs.core.MediaType;
import jakarta.ws.rs.core.Response;

@Path("entry")
public class EntryResource {

	@Path("forbidden")
	@GET
	public Response forbidden() {
		return Response.status(403).build();
	}

	@GET
	@Path("success")
	public Response success() {
		return Response.ok().build();
	}

	@HEAD
	@Path("success")
	public Response successHead() {
		return Response.ok().build();
	}

	@HEAD
	@Path("failed")
	public Response failed() {
		return Response.status(400).build();
	}

	@OPTIONS
	@Path("success")
	public Response options() {
		return Response.ok("GET,HEAD").build();
	}

	@OPTIONS
	@Path("failed")
	public Response notAllowed() {
		return Response.status(405).entity("OPTIONS is not allowed").build();
	}

	@PATCH
	@Path("success")
	@Produces(MediaType.TEXT_PLAIN)
	public Response patchSuccess() {
		return Response.ok("ok").build();

	}

	@PATCH
	@Path("failed")
	public Response patchFailed() {
		return Response.status(400).build();
	}

	@PUT
	@Path("success")
	public Response putSuccess(String data) {
		return Response.status("Hello ILEvator!".equals(data) ? 200 : 400).entity("ok").build();
	}

	@PUT
	@Path("failed")
	public Response putFailed(String data) {
		return Response.status(400).build();
	}

	@DELETE
	@Path("success")
	public Response deleteSuccess() {
		return Response.noContent().build();
	}

	@DELETE
	@Path("notallowed")
	public Response deleteNotAllowed() {
		return Response.status(405).entity("Method not allowed").build();
	}

}
