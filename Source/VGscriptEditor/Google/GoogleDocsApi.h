// Copyright (c) Noümen Interactive. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "GoogleDocsApi.generated.h"

/**
 * 
 */
UCLASS()
class VGSCRIPTEDITOR_API UGoogleDocsApi : public UObject
{
	GENERATED_BODY()

	DECLARE_DELEGATE_OneParam(OnResponse, FString);
	
public:	
	OnResponse OnResponseDelegate;

	void SendRequest(FString docId);

private:
	static const FString ApiBaseUrl;
	FHttpModule* Http;

	void ProcessResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void SetRequestHeaders(TSharedRef<IHttpRequest>& Request);
	bool ResponseIsValid(FHttpResponsePtr Response, bool bWasSuccessful);

	TSharedRef<IHttpRequest> GetRequest(FString Subroute);
	TSharedRef<IHttpRequest> PostRequest(FString Subroute, FString ContentJsonString);
	TSharedRef<IHttpRequest> RequestWithRoute(FString Subroute);

	UFUNCTION(BlueprintCallable)
	void FetchGoogleDocPlainText();

	FString GetTextFromHTML(const FString& HTMLContent);
};