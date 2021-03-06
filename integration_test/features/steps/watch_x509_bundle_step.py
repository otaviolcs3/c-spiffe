import os
import time

from hamcrest import assert_that, is_not


@given('I set the server to rotate the Bundle up to "{time}"')
def step_impl(context, time):
    os.system("/mnt/c-spiffe/integration_test/helpers/bash-spire-scripts/ssh-update-server-conf.sh default_svid_ttl %s server" % time)


@when('I store the "{document}"')
def step_impl(context, document):
    exec("context.current_{0} = context.{0}".format(document.lower()))


@then('The "{document}" was updated')
def step_impl(context, document):
    document = document.lower()
    exec("assert_that(context.%s, is_not('(nil)'), 'Document is empty: (nil)')" % document)
    exec("assert_that(context.{0}, is_not(context.current_{0}), 'Document was not updated.')".format(document))


@when('The server is turned off')
def step_impl(context):
    os.system("pkill spire-server")
    time.sleep(5)
    

@when('The server is turned on')
def step_impl(context):
    os.system("/mnt/c-spiffe/integration_test/helpers/bash-spire-scripts/ssh-start-server.sh")
    time.sleep(5)
