namespace Client
{
    public partial class JoinRoomWindow : System.Windows.Window
    {
        private bool _isRoomClicked = false;
        private uint _clickedRoomId = 0;
        private string _roomName = "";
        private List<Tuple<System.Windows.Controls.Button, uint>> _buttonList = new List<Tuple<System.Windows.Controls.Button, uint>>();
        private System.Threading.Thread _thread = null;
        private System.Threading.CancellationTokenSource _cancellationTokenSource = new System.Threading.CancellationTokenSource();

        public JoinRoomWindow()
        {
            this.InitializeComponent();
            this._thread = new System.Threading.Thread(new System.Threading.ThreadStart(Automatic_Refresh));
            this._thread.Start();
        }

        public void SwitchInit()
        {
            this._cancellationTokenSource = new System.Threading.CancellationTokenSource();
            this._thread = new System.Threading.Thread(new System.Threading.ThreadStart(Automatic_Refresh));
            this._thread.Start();
        }

        public void Click_Refresh(object sender, System.Windows.RoutedEventArgs e)
        {
            lock (this)
            {
                // Send a request to the server to get all Rooms.
                // After getting all the rooms it prints all the rooms on the screen.

                // Remove all the previous buttons.
                RoomPanel.Children.Clear();
                this._buttonList.Clear();

                ClientSocket.SendMessage(Constants.Client.GET_ROOMS);
                System.Text.Json.Nodes.JsonObject j = ClientSocket.ReceiveMessage().Item2.AsObject();

                int roomsNum = j["ids"].AsArray().Count; // The amount of IDs is the amount of the other rooms I could have chosen any other element.

                // Extract each button and print on the screen.
                for (int i = 0; i < roomsNum; i++)
                {
                    System.Windows.Controls.Button button = new System.Windows.Controls.Button();

                    this._buttonList.Add(new Tuple<System.Windows.Controls.Button, uint>(button, (uint)j["ids"][i]));

                    button.Width = 200;
                    button.Height = 50;
                    button.VerticalAlignment = System.Windows.VerticalAlignment.Top;
                    button.HorizontalAlignment = System.Windows.HorizontalAlignment.Center;

                    button.Margin = new System.Windows.Thickness(0, 10, 0, 0);
                    button.Content = j["names"][i];
                    button.FontFamily = new System.Windows.Media.FontFamily("Comic Sans MS");
                    button.Click += Click_Room;

                    if ((uint)j["ids"][i] == this._clickedRoomId)
                    {
                        button.Background = new System.Windows.Media.SolidColorBrush(System.Windows.Media.Color.FromRgb(173, 216, 230));
                    }
                    else
                    {
                        button.Background = new System.Windows.Media.SolidColorBrush(System.Windows.Media.Colors.White);
                    }

                    RoomPanel.Children.Add(button);
                }
            }
        }
        public void Click_Room(object sender, System.Windows.RoutedEventArgs e)
        {
            lock (this)
            {
                // Get button clicked ID.
                // Will be used on Click_Join() with that ID.

                if (this._isRoomClicked)
                {
                    foreach (System.Tuple<System.Windows.Controls.Button, uint> item in this._buttonList)
                    {
                        item.Item1.Background = new System.Windows.Media.SolidColorBrush(System.Windows.Media.Colors.White);
                    }
                }

                System.Windows.Controls.Button button = sender as System.Windows.Controls.Button; // Cast to System.Windows.Controls.Button.

                foreach (System.Tuple<System.Windows.Controls.Button, uint> item in this._buttonList)
                {
                    if (item.Item1.Content == button.Content)
                    {
                        button.Background = new System.Windows.Media.SolidColorBrush(System.Windows.Media.Color.FromRgb(173, 216, 230));
                        this._isRoomClicked = true;
                        this._clickedRoomId = item.Item2;
                        this._roomName = item.Item1.Content.ToString();
                    }
                }
            }
        }
        public void Click_Join(object sender, System.Windows.RoutedEventArgs e)
        {
            if (this._isRoomClicked)
            {
                this.StopThread();

                System.Text.Json.Nodes.JsonObject j = new System.Text.Json.Nodes.JsonObject();
                j["roomId"] = this._clickedRoomId;
                ClientSocket.SendMessage(Constants.Client.JOIN_ROOM, j);
                byte code = ClientSocket.ReceiveMessage().Item1;

                if (code != Constants.Server.FAILURE)
                {
                    RoomPanel.Children.Clear();
                    this._buttonList.Clear();
                    ClientHelper.SwitchToNewWindow(this, new RoomWindow(this._roomName));
                }
            }
        }

        private void Automatic_Refresh()
        {
            while (!this._cancellationTokenSource.Token.IsCancellationRequested)
            {
                try
                {
                    this.Dispatcher.Invoke(() => Click_Refresh(null, null));
                }
                catch {}
                ClientHelper.CustomSleep(3000, this._cancellationTokenSource);
            }
        }

        private void StopThread()
        {
            this._cancellationTokenSource.Cancel();

            if (this._thread != null && this._thread.IsAlive)
            {
                this._thread.Join();
            }
        }
        private void Click_GoBack(object sender, System.Windows.RoutedEventArgs e)
        {
            this.StopThread();

            ClientHelper.SwitchToParentWindow(this);
        }
        protected override void OnClosed(System.EventArgs e)
        {
            if (ClientHelper.doClose)
            {
                ClientSocket.Destructor();
                base.OnClosed(e);
                System.Environment.Exit(0);
            }
            ClientHelper.doClose = true;
        }
    }
}
