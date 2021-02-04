
#include "watcher.h"
#include "threads.h"

//Function that will run on thread spun for watcher
int workloadapi_Watcher_X509backgroundFunc(void * _watcher){
    workloadapi_Watcher* watcher = (workloadapi_Watcher*) _watcher;

    err_t error = NO_ERROR;
    
    ///TODO: implement on client. should only return on exit.
    //error = workloadapi_Client_WatchX509Context(watcher->client,watcher); 

    return (int) error;
} 



//new watcher, dials WorkloadAPI if client hasn't yet
workloadapi_Watcher* workloadapi_newWatcher(workloadapi_WatcherConfig config, workloadapi_X509Callback x509Callback/*, jwtBundleSetFunc_t* jwtBundleSetUpdateFunc*/, err_t* error){
    workloadapi_Watcher* newW = (workloadapi_Watcher*) calloc(1,sizeof *newW);
    if(config.client){
        newW->client = config.client;
        newW->ownsClient = false;
        ///TODO: apply client options to client?
    }
    else{
        ///TODO: init client with constructor.
        newW->client = NULL; // = newClient(config.clientOptions);
    }

    // set by calloc:
    // newW->updated = false;
    // newW->closed = false;
    // newW->closeError = NO_ERROR;
    // newW->updateError = NO_ERROR;


    mtx_init(&(newW->closeMutex),mtx_plain);
    mtx_init(&(newW->updateMutex),mtx_plain);
    cnd_init(&(newW->updateCond));

    ///TODO: check if callback is valid?
    /// set callback and spin thread out.
    newW->x509Callback = x509Callback;
    int thread_error = thrd_create(&(newW->watcherThread),workloadapi_Watcher_X509backgroundFunc,newW);
    
    // if (thread_error != thrd_success){
    //     ///TODO: check error on thread creation
         
    // }


    ///wait for update and check for errors.
    *error = workloadapi_Watcher_WaitUntilUpdated(newW);
    ///TODO: add error checking.
    //if(error){

    // }
    return newW;
}

//drops connection to WorkloadAPI (if owns client)
err_t workloadapi_closeWatcher(/*context,*/ workloadapi_Watcher* watcher){
    mtx_lock(&(watcher->closeMutex));
    watcher->closed = true;
    ///TODO: check and set watcher->closeError?
    ///TODO: notify thread of closure;
    
    mtx_unlock(&(watcher->closeMutex));

    return NO_ERROR;
}

//Function called by Client when new x509 response arrives
void workloadapi_Watcher_OnX509ContextUpdate(workloadapi_Watcher* watcher, workloadapi_X509Context* context){
    void *args = watcher->x509Callback.args;
    watcher->x509Callback.func(context,args);
}

//Called by Client when an error occurs
void workloadapi_Watcher_OnX509ContextWatchError(workloadapi_Watcher* watcher, err_t error){
    ///TODO: catch/recover/exit from watch error 
    ///INFO: go-spiffe does nothing.
}

err_t workloadapi_Watcher_WaitUntilUpdated(workloadapi_Watcher* watcher){
    mtx_lock(&watcher->updateMutex);
    if (watcher->updated){
        mtx_unlock(&watcher->updateMutex);
        return NO_ERROR;
    }
    else{ ///TODO: should we wait on a timer?
        cnd_wait(&(watcher->updateCond), &(watcher->updateMutex));
    }
    mtx_unlock(&watcher->updateMutex);
}