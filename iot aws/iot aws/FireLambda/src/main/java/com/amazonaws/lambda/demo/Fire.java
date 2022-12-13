package com.amazonaws.lambda.demo;

import com.amazonaws.auth.AWSStaticCredentialsProvider;
import com.amazonaws.auth.BasicAWSCredentials;
import com.amazonaws.regions.Regions;
import com.amazonaws.services.lambda.runtime.Context;
import com.amazonaws.services.lambda.runtime.RequestHandler;
import com.amazonaws.services.sns.AmazonSNS;
import com.amazonaws.services.sns.AmazonSNSClientBuilder;
import com.amazonaws.services.sns.model.PublishRequest;
import com.amazonaws.services.sns.model.PublishResult;
import com.google.gson.JsonElement;
import com.google.gson.JsonParser;

public class Fire implements RequestHandler<Object, String> {
	@Override
	public String handleRequest(Object input, Context context) {
	    context.getLogger().log("Input: " + input);
	    String json = ""+input;
	    JsonParser parser = new JsonParser();
	    JsonElement element = parser.parse(json);
	    JsonElement state = element.getAsJsonObject().get("state");
	    JsonElement reported = state.getAsJsonObject().get("reported");
	    String gas = reported.getAsJsonObject().get("gas").getAsString();
	    String temp = reported.getAsJsonObject().get("temperature").getAsString();
	    double g = Double.valueOf(gas);
	    double t = Double.valueOf(temp);

	    final String AccessKey="AKIAYNZYVK7ZTDKRRA2O";
	    final String SecretKey="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";
	    final String topicArn="arn:aws:sns:ap-northeast-1:579402553331:Fire_Warning";

	    BasicAWSCredentials awsCreds = new BasicAWSCredentials(AccessKey, SecretKey);  
	    AmazonSNS sns = AmazonSNSClientBuilder.standard()
	                .withRegion(Regions.AP_NORTHEAST_1)
	                .withCredentials( new AWSStaticCredentialsProvider(awsCreds) )
	                .build();

	    final String msg = "*Fire Warning!*\n";
	    final String subject = "Fire Warning!";
	    if (g >= 700.0 || t >= 36.0) {
	    	PublishRequest publishRequest = new PublishRequest(topicArn, msg, subject);
	        PublishResult publishResponse = sns.publish(publishRequest);
	    }
		return subject;

	
	}
}
