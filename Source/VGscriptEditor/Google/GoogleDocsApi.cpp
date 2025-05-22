// Copyright (c) Noümen Interactive. All Rights Reserved.

#include "GoogleDocsApi.h"

const FString UGoogleDocsApi::ApiBaseUrl = "https://docs.google.com/document/d/";

void UGoogleDocsApi::SendRequest(FString docId)
{
	TSharedRef<IHttpRequest> Request = GetRequest(FString::Printf(TEXT("%s/export?format=csv"), *docId));
	Request->OnProcessRequestComplete().BindUObject(this, &UGoogleDocsApi::ProcessResponse);
	Request->ProcessRequest();
}

void UGoogleDocsApi::ProcessResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!ResponseIsValid(Response, bWasSuccessful)) return;

	OnResponseDelegate.ExecuteIfBound(Response->GetContentAsString());
}

TSharedRef<IHttpRequest> UGoogleDocsApi::RequestWithRoute(FString Subroute)
{
	Http = &FHttpModule::Get();
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->SetURL(UGoogleDocsApi::ApiBaseUrl + Subroute);
	SetRequestHeaders(Request);
	return Request;
}

void UGoogleDocsApi::SetRequestHeaders(TSharedRef<IHttpRequest>& Request)
{
	Request->SetHeader(TEXT("User-Agent"), TEXT("X-UnrealEngine-Agent"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("text/csv"));
	Request->SetHeader(TEXT("Accepts"), TEXT("text/csv"));
}

TSharedRef<IHttpRequest> UGoogleDocsApi::GetRequest(FString Subroute)
{
	TSharedRef<IHttpRequest> Request = RequestWithRoute(Subroute);
	Request->SetVerb("GET");
	return Request;
}

TSharedRef<IHttpRequest> UGoogleDocsApi::PostRequest(FString Subroute, FString ContentJsonString)
{
	TSharedRef<IHttpRequest> Request = RequestWithRoute(Subroute);
	Request->SetVerb("POST");
	Request->SetContentAsString(ContentJsonString);
	return Request;
}

bool UGoogleDocsApi::ResponseIsValid(FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful || !Response.IsValid()) return false;
	if (EHttpResponseCodes::IsOk(Response->GetResponseCode())) return true;
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Http Response returned error code: %d"), Response->GetResponseCode());
		return false;
	}
}

void UGoogleDocsApi::FetchGoogleDocPlainText()
{
	FString AccessToken = "YOUR_ACCESS_TOKEN"; // Replace with your access token
	FString DocumentId = "10vMCr6dbVkKiXv6Qkz9b1GSb8russzLJ8L5tYJpiZFk"; // Replace with the ID of your Google Doc

	// Construct the URL to fetch the document content
	FString Url = FString::Printf(
		TEXT("https://www.googleapis.com/drive/v3/files/%s/export?mimeType=text/plain"),
		*DocumentId);
	Url = "https://docs.google.com/document/d/10vMCr6dbVkKiXv6Qkz9b1GSb8russzLJ8L5tYJpiZFk";

	// Construct the HTTP request
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(Url);
	HttpRequest->SetVerb("GET");
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("text/javascript"));
	HttpRequest->SetHeader(TEXT("Accepts"), TEXT("text/javascript"));
	//HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *AccessToken));

	// Define the completion handler
	HttpRequest->OnProcessRequestComplete().BindLambda(
		[this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
		{
			if (bSuccess && Response.IsValid() && Response->GetResponseCode() == EHttpResponseCodes::Ok)
			{
				FString DocumentContent = Response->GetContentAsString();
				//FString Content = GetTextFromHTML(DocumentContent);
				UE_LOG(LogTemp, Warning, TEXT("Google Doc Content: %s"), *DocumentContent);

				// Now you can process the document content as needed
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to fetch Google Doc content."));
			}
		});

	// Send the HTTP request
	HttpRequest->ProcessRequest();
}

FString UGoogleDocsApi::GetTextFromHTML(const FString& HTMLContent)
{
	// Find the start and end tags indicating the text content
	int32 StartIndex = HTMLContent.Find("<body>");
	int32 EndIndex = HTMLContent.Find("</body>");

	if (StartIndex != INDEX_NONE && EndIndex != INDEX_NONE && StartIndex < EndIndex)
	{
		// Extract the text content between the start and end tags
		FString TextContent = HTMLContent.Mid(StartIndex + 6, EndIndex - StartIndex - 6);
		// Adjust offsets based on tag lengths

		// You may need to perform further processing to clean up the text
		// For example, remove HTML tags, special characters, etc.

		return TextContent;
	}

	return FString(); // Return empty string if text content not found
}