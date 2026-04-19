static class ClientSocket
{
    private static readonly string DefaultServerIP = "127.0.0.1";
    private static readonly int DefaultServerPort = 8826;

    private static System.Net.Sockets.TcpClient _server;
    private static System.Net.Sockets.NetworkStream _stream;

    // A constructor is a called only when one of the methods is called not when the programs starts.
    // This creates some problems and it's a must to use force Init.
    public static bool Initialize()
    {
        try
        {
            ClientSocket._server = new System.Net.Sockets.TcpClient(ClientSocket.DefaultServerIP, ClientSocket.DefaultServerPort);
            ClientSocket._stream = ClientSocket._server.GetStream();
            return true;
        }
        catch
        {
            return false;
        }
    }
    public static void Destructor()
    {
        try
        {
            ClientSocket._stream.Close();
            ClientSocket._server.Close();
        }
        catch { }
    }

    public static void SendMessage(byte code, System.Text.Json.Nodes.JsonObject json)
    {
        try
        {
            string message = json.ToString();
            byte[] data = new byte[sizeof(byte) + sizeof(uint) + message.Length];
            byte[] lengthBytes = new byte[4];

            // Enforce little endian.
            lengthBytes[0] = (byte)(message.Length);
            lengthBytes[1] = (byte)(message.Length >> 8); // SHR [message.Length], 8
            lengthBytes[2] = (byte)(message.Length >> 16); // SHR [message.Length], 16
            lengthBytes[3] = (byte)(message.Length >> 24); // SHR [message.Length], 24

            System.Array.Copy(new byte[] { code }, 0, data, 0, sizeof(byte));
            Array.Copy(lengthBytes, 0, data, sizeof(byte), sizeof(uint));
            System.Array.Copy(System.Text.Encoding.ASCII.GetBytes(message), 0, data, sizeof(byte) + sizeof(uint), message.Length);

            ClientSocket._stream.Write(data, 0, data.Length);
        }
        catch
        {
            ClientHelper.ForceRestart();
        }
    }
    public static void SendMessage(byte code)
    {
        try
        {
            byte[] data = new byte[sizeof(byte) + sizeof(uint)];
            byte[] lengthBytes = new byte[4];

            lengthBytes[0] = 0;
            lengthBytes[1] = 0;
            lengthBytes[2] = 0;
            lengthBytes[3] = 0;

            System.Array.Copy(new byte[] { code }, 0, data, 0, sizeof(byte));
            Array.Copy(lengthBytes, 0, data, sizeof(byte), sizeof(uint));

            ClientSocket._stream.Write(data, 0, data.Length);
        }
        catch
        {
            ClientHelper.ForceRestart();
        }
    }

    public static Tuple<byte, System.Text.Json.Nodes.JsonObject> ReceiveMessage()
    {
        try
        {
            byte[] staticBuffer = new byte[sizeof(byte) + sizeof(uint)]; // Not really static but it's just a way to convay it.
            ClientSocket._stream.Read(staticBuffer, 0, staticBuffer.Length);
            byte code = staticBuffer[0];
            uint stringLength = System.BitConverter.ToUInt32(staticBuffer, sizeof(byte));

            byte[] dynamicBuffer = new byte[stringLength];
            ClientSocket._stream.Read(dynamicBuffer, 0, dynamicBuffer.Length);

            string convertedString = System.Text.Encoding.ASCII.GetString(dynamicBuffer);
            System.Text.Json.Nodes.JsonObject json = System.Text.Json.Nodes.JsonNode.Parse(convertedString).AsObject();

            if (json["status"].ToString() == "0")
            {
                ClientHelper.Popup(json["message"].ToString());
            }

            return new Tuple<byte, System.Text.Json.Nodes.JsonObject>(code, json);
        }
        catch
        {
            ClientHelper.ForceRestart();
            return null; // Will not take effect, Just to make the Compiler to shut up.
        }
    }
}