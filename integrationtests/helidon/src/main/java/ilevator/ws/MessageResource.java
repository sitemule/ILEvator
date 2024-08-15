package ilevator.ws;

import java.util.logging.Logger;

import jakarta.inject.Inject;
import jakarta.ws.rs.Consumes;
import jakarta.ws.rs.POST;
import jakarta.ws.rs.Path;
import jakarta.ws.rs.core.MediaType;
import jakarta.ws.rs.core.Response;

@Path("message")
public class MessageResource {

	private final Logger logger = Logger.getLogger(this.getClass().getName());

	@Inject
	private ObservableQueue<String> messageQueue;

	@POST
	@Consumes(MediaType.TEXT_PLAIN)
	public Response postMessage(String message) {
		messageQueue.add(message);
		logger.info("Message count: " + messageQueue.size());

		return Response.noContent().build();
	}

}
