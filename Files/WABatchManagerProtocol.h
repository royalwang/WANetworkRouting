//
//  WABatchManagerProtocol.h
//  WANetworkRouting
//
//  Created by Marian Paul on 29/03/2016.
//  Copyright © 2016 Wasappli. All rights reserved.
//

@import Foundation;

@class WABatchSession, WAObjectRequest, WAObjectResponse, WABatchResponse;
@protocol WANRErrorProtocol, WABatchManagerDelegate, WABatchManagerProtocol;

/**
 *  A block called on success
 *
 *  @param batchManager   the batch manager triggering the event
 *  @param batchResponses an array of batch responses which contains the request, the response and the mapped object if the mapping is configured
 */
typedef void (^WABatchManagerSuccess)(_Nonnull id <WABatchManagerProtocol> batchManager, NSArray <WABatchResponse *> *_Nullable batchResponses);

/**
 *  A block called on failure
 *
 *  @param batchManager             the batch manager triggering the event
 *  @param request                  the batch request
 *  @param response                 the response from the batch request
 *  @param error                    the error
 */
typedef void (^WABatchManagerFailure)(_Nonnull id <WABatchManagerProtocol> batchManager, WAObjectRequest *_Nonnull request, WAObjectResponse *_Nullable response, _Nullable id <WANRErrorProtocol>error);

/**
 This protocols defines the batch manager. The library has a default implementation `WABatchManager`.
 The idea of the protocol is to send a batch session, but also allow some requests to be automatically enqueued if the app is offline, and then enqueued if the app comes online.
*/
@protocol WABatchManagerProtocol <NSObject>

/**
 *  Init the batch manager with a path and a limit.
 *  The limit is the batch size your server can receive. If the number of objects to send is greater than the limit, then the flush will happen in several steps.
 *
 *  @param batchPath the path to reach the batch relative to base url
 *  @param limit     the batch limit your server has
 *
 *  @return a batch manager
 */
- (instancetype _Nonnull)initWithBatchPath:(NSString *_Nonnull)batchPath limit:(NSUInteger)limit;

/**
 *  @see `initWithBatchPath: limit`
 *
 *  @param batchPath the path to reach the batch relative to base url
 *  @param limit     the batch limit your server has
 *
 *  @return a batch manager
 */
+ (instancetype _Nonnull)batchManagerWithBatchPath:(NSString *_Nonnull)batchPath limit:(NSUInteger)limit;

// ———————————————————————
// Offline management
// ———————————————————————

/**
 *  Ask the batch manager if it needs to flush some data enqueued after offline errors
 *
 *  @return YES if elements needs to be flushed AND we are not flushing
 */
- (BOOL)needsFlushing;

/**
 *  Flush the batch data we have from offline request enqueued and synchronise with the server
 *
 *  @param completion a completion block called when the flush is complete.
 */
- (void)flushDataWithCompletion:( void(^ _Nonnull )(BOOL success))completion;

/**
 *  Determine if a request can be enqueued or not. On `WABatchManager` implementation, you have to use `WANetworkRoute`
 *
 *  @param request the request which cannot be triggered because the app is offline
 *  @param response the optional response
 *  @param error the optional error
 *
 *  @return YES if the request can be enqueued
 */
- (BOOL)canEnqueueOfflineRequest:(WAObjectRequest *_Nonnull)request withResponse:(WAObjectResponse *_Nullable)response error:(_Nullable id<WANRErrorProtocol>) error;

/**
 *  Enqueue for offline the request which cannot be triggered because the app is offline
 *
 *  @param request the request to enqueue
 */
- (void)enqueueOfflineRequest:(WAObjectRequest *_Nonnull)request;

/**
 *  Reset the database and remove all saved requests
 */
- (void)reset;

/**
 *  Return the flush status. If a flush is in progress, you should not send request but batch them waiting for the batch end so that the reconciliation with the server can be correct
 *
 *  @return YES if a flush is in progress, NO otherwise
 */
- (BOOL)isFlushing;

// ———————————————————————
// Classic batch
// ———————————————————————

/**
 *  Send a batch session. Use this method if you want to perform multiple calls at the same time. For example: getting the events, the user profile, the news list after a signin. You would create a session with three GET requests
 *
 *  @param session      the session with the batched requests
 *  @param successBlock a block called on success
 *  @param failureBlock a block called on failure
 */
- (void)sendBatchSession:(WABatchSession *_Nonnull)session successBlock:(_Nullable WABatchManagerSuccess)successBlock failureBlock:(_Nullable WABatchManagerFailure)failureBlock;

/**
 *  A delegate for the batch. Used for communicating with the `WANetworkRoutingManager` to enqueue the request and map the responses
 */
@property (nonatomic, weak) _Nullable id <WABatchManagerDelegate> delegate;

/**
 *  A block called on each offline batch session sent if success. Basically, you should call `neesdFlushing` to know if there are some pending offline batch objects to send.
 */
@property (nonatomic, copy) _Nullable WABatchManagerSuccess offlineFlushSuccessBlock;
/**
 *  A block called on offline batch if there is any error. Note that this is the `/batch` error, not the possible errors from the batch requests themselves.
 */
@property (nonatomic, copy) _Nullable WABatchManagerFailure offlineFlushFailureBlock;

@end

@protocol WABatchManagerDelegate <NSObject>

- (void)batchManager:(_Nonnull id<WABatchManagerProtocol>)batchManager haveBatchRequestToEnqueue:(WAObjectRequest *_Nonnull)objectRequest;
- (void)batchManager:(_Nonnull id<WABatchManagerProtocol>)batchManager haveBatchResponsesToProcess:(NSArray <WABatchResponse *>*_Nonnull)batchReponses;

@end
