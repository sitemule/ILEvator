package ilevator;

import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.nio.file.StandardCopyOption;
import java.util.logging.Logger;

import org.eclipse.microprofile.config.inject.ConfigProperty;
import org.glassfish.jersey.media.multipart.FormDataBodyPart;
import org.glassfish.jersey.media.multipart.FormDataContentDisposition;
import org.glassfish.jersey.media.multipart.FormDataParam;

import jakarta.inject.Inject;
import jakarta.ws.rs.Consumes;
import jakarta.ws.rs.POST;
import jakarta.ws.rs.Path;
import jakarta.ws.rs.core.MediaType;
import jakarta.ws.rs.core.Response;

@Path("multipart")
public class MultiPartResource {

	private final Logger logger = Logger.getLogger(this.getClass().getName());

	@Inject
	@ConfigProperty(name = "ilevator.upload.dir")
	String uploadDirectory;

	@POST
	@Path("form")
	@Consumes(MediaType.MULTIPART_FORM_DATA)
	public Response postFormDataTextOnly(@FormDataParam("user") FormDataBodyPart username,
			@FormDataParam("email") FormDataBodyPart email) {
		return Response.ok(username.getValue() + " - " + email.getValue()).build();
	}

	@POST
	@Path("file")
	@Consumes(MediaType.MULTIPART_FORM_DATA)
	public Response postFileAndText(@FormDataParam("user") FormDataBodyPart username,
			@FormDataParam("avatar") InputStream avatarIconStream,
			@FormDataParam("avatar") FormDataContentDisposition avatarIcon) {
		logger.info("Avatar icon upload for user " + username.getValue());
		logger.info("Avatar icon upload filename: " + avatarIcon.getFileName());

		java.nio.file.Path downloadPath = Paths.get(uploadDirectory, avatarIcon.getFileName());

		try {
			Files.copy(avatarIconStream, downloadPath, StandardCopyOption.REPLACE_EXISTING);
			return Response.ok().build();
		} catch (IOException e) {
			e.printStackTrace();
			return Response.serverError().entity(e.getMessage()).build();
		}

	}

}
