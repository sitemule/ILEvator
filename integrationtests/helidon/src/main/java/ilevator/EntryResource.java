package ilevator;

import javax.ws.rs.DELETE;
import javax.ws.rs.GET;
import javax.ws.rs.HEAD;
import javax.ws.rs.OPTIONS;
import javax.ws.rs.PATCH;
import javax.ws.rs.PUT;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;

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
