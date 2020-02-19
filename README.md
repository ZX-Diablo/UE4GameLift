# UE4GameLift

Unreal Engine 4 and Amazon GameLift integration. For more info see related [blog article](https://artaxind.wordpress.com/2020/02/19/integrating-unreal-engine-with-amazon-gamelift-using-c/).

## Description

_UE4GameLift_ is a sample program  that demonstrates integration with Amazon GameLift service.
Windows dedicated server and windows client are implemented.
The server hosts level and accepts clients. The client can create new session or join the existing one.

AWS SDK and Unreal Engine update more often than required plugins, so solutions here could be insufficient or outdated.
Keep in mind that your implementation could require additional steps.

## Building

Built with Unreal Engine 4.24.1 using Microsoft Visual Studio Community 2017 (v15.9.16).

## Installation

Not required but you'll need GameLift Local for local testing or set up Amazon account for remote.

## Usage

To test locally: launch GameLift Local, launch the server, launch the client with the local flag.
Type in the console on the client command "HostGameSession" to create a new game session and connect to it on the server.
You can launch another client and type command "JoinGameSession" with a session ID (get it from the logs) to join the same game on the server.

To test on Amazon you'll need to create an account, configure and build project with your credentials (or just edit packaged client config),
upload build, create fleet and then test with clients without local flag using the same steps.
