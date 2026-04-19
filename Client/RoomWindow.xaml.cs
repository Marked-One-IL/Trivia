namespace Client
{
    public partial class RoomWindow : System.Windows.Window
    {
        bool _isAdmin = false;
        bool _roomClose = false;
        bool _isGameStarted = false;
        bool _firsdTimeInGame = true;
        private System.Threading.Thread _thread = null;
        private System.Threading.CancellationTokenSource _cancellationTokenSource = new System.Threading.CancellationTokenSource();
        private int _questionCount;

        public RoomWindow(string room_name)
        {
            this.InitializeComponent();

            roomName.Content = "Room: " + room_name;

            ClientSocket.SendMessage(Constants.Client.GET_ROOM_STATE);
            System.Text.Json.Nodes.JsonObject json_init = ClientSocket.ReceiveMessage().Item2;
            maxPlayers.Content = "Max players: " + json_init["maxPlayers"].ToString();
            timePerQuestion.Content = "Time per question: " + json_init["answerTimeout"].ToString();
            questionCount.Content = "Question count: " + json_init["questionCount"].ToString();
            this._questionCount = int.Parse(json_init["questionCount"].ToString());

            ClientHelper.timePerQuesion = int.Parse(json_init["answerTimeout"].ToString());

            ClientSocket.SendMessage(Constants.Client.GET_ROOM_STATE);
            System.Text.Json.Nodes.JsonObject json = ClientSocket.ReceiveMessage().Item2;

            byte roomStatus = byte.Parse(json["roomStatus"].ToString());

            if (roomStatus == Constants.Room_Status.CLOSED)
            {
                this._cancellationTokenSource.Cancel();
                this._roomClose = true;
                return;
            }

            System.Text.Json.Nodes.JsonArray players = json["players"].AsArray();

            PlayerCanvas.Children.Clear();

            for (int i = 0; i < players.Count; i++)
            {
                System.Windows.Controls.Label user = new System.Windows.Controls.Label();
                if (i == 0)
                {
                    user.Content = "Room Admin - " + players[i].ToString();

                    if (ClientHelper.current_user == players[i].ToString())
                    {
                        this._isAdmin = true;
                        GoBackButton.Visibility = System.Windows.Visibility.Collapsed;
                        CloseRoomButton.Visibility = System.Windows.Visibility.Visible;
                    }
                }
                else
                {
                    user.Content = players[i].ToString();
                    GoBackButton.Visibility = System.Windows.Visibility.Visible;
                }
                user.FontSize = 30;
                user.VerticalAlignment = System.Windows.VerticalAlignment.Center;
                user.HorizontalAlignment = System.Windows.HorizontalAlignment.Left;
                user.Margin = new System.Windows.Thickness(0, 10, 0, 0);
                user.FontFamily = new System.Windows.Media.FontFamily("Comic Sans MS");
                user.Visibility = System.Windows.Visibility.Visible;

                PlayerCanvas.Children.Add(user);
            }

            if (this._isAdmin)
            {
                StartGameButton.Visibility = System.Windows.Visibility.Visible;
            }
            else
            {
                RefreshButton.Margin = new System.Windows.Thickness(245, 655, 0, 0);
            }

            RefreshButton.Visibility = System.Windows.Visibility.Visible;

            this._thread = new System.Threading.Thread(Automatic_Refresh);
            this._thread.Start();
        }

        private void Click_Refresh(object sender, System.Windows.RoutedEventArgs e)
        {
            lock (this)
            {
                ClientSocket.SendMessage(Constants.Client.GET_ROOM_STATE);
                System.Text.Json.Nodes.JsonObject json = ClientSocket.ReceiveMessage().Item2;

                byte roomStatus = byte.Parse(json["roomStatus"].ToString());

                if (roomStatus == Constants.Room_Status.CLOSED)
                {
                    this._cancellationTokenSource.Cancel();
                    this._roomClose = true;

                    if (!this._isAdmin)
                    {
                        ClientHelper.Popup("The Admin closed/left the game");
                    }

                    return;
                }
                else if (roomStatus == Constants.Room_Status.IN_GAME && this._firsdTimeInGame && !this._isAdmin)
                {
                    this._firsdTimeInGame = false;
                    this._isGameStarted = true;
                    this._cancellationTokenSource.Cancel();
                    return;
                }

                System.Text.Json.Nodes.JsonArray players = json["players"].AsArray();
                PlayerCanvas.Children.Clear();

                for (int i = 0; i < players.Count; i++)
                {
                    System.Windows.Controls.Label user = new System.Windows.Controls.Label();
                    if (i == 0)
                    {
                        user.Content = "Room Admin - " + players[i].ToString();
                    }
                    else
                    {
                        user.Content = players[i].ToString();
                        GoBackButton.Visibility = System.Windows.Visibility.Visible;
                    }
                    user.FontSize = 30;
                    user.VerticalAlignment = System.Windows.VerticalAlignment.Center;
                    user.HorizontalAlignment = System.Windows.HorizontalAlignment.Left;
                    user.Margin = new System.Windows.Thickness(0, 10, 0, 0);
                    user.FontFamily = new System.Windows.Media.FontFamily("Comic Sans MS");
                    user.Visibility = System.Windows.Visibility.Visible;

                    PlayerCanvas.Children.Add(user);
                }
            }
        }

        private void Click_StartGame(object sender, System.Windows.RoutedEventArgs e)
        {
            lock (this)
            {
                this._isGameStarted = true;
                this.StopThread();
                ClientSocket.SendMessage(Constants.Client.START_GAME);
                ClientSocket.ReceiveMessage();
                ClientHelper.SwitchToNewWindow(this, new Client.GameWindow(this._questionCount));
                return;
            }
        }

        private void Automatic_Refresh(object obj)
        {
            while (!this._cancellationTokenSource.Token.IsCancellationRequested)
            {
                try
                {
                    this.Dispatcher.Invoke(() => Click_Refresh(null, null));
                }
                catch {}

                if (this._isGameStarted) 
                {
                    break;
                }
                ClientHelper.CustomSleep(1000, this._cancellationTokenSource);
            }

            if (!this._isAdmin && this._roomClose)
            {
                this.Dispatcher.Invoke(() =>
                {
                    ClientHelper.SwitchToParentWindow(this);
                });
            }
            else if (!this._isAdmin && this._isGameStarted)
            {
                this.Dispatcher.Invoke(() =>
                {
                    ClientHelper.SwitchToNewWindow(this, new Client.GameWindow(this._questionCount));
                });
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

        private void Click_CloseRoom(object sender, System.Windows.RoutedEventArgs e)
        {
            this.StopThread();

            ClientSocket.SendMessage(Constants.Client.CLOSE_ROOM);
            ClientSocket.ReceiveMessage();

            ClientHelper.Popup("You closed the game");
            ClientHelper.SwitchToParentWindow(this);
        }
        private void Click_GoBack(object sender, System.Windows.RoutedEventArgs e)
        {
            this.StopThread();

            ClientSocket.SendMessage(Constants.Client.LEAVE_ROOM);
            ClientSocket.ReceiveMessage();
            ClientHelper.SwitchToParentWindow(this);
        }

        protected override void OnClosed(System.EventArgs e)
        {
            if (ClientHelper.doClose)
            {
                this.StopThread();

                if (this._isAdmin)
                {
                    ClientSocket.SendMessage(Constants.Client.CLOSE_ROOM);
                    ClientSocket.ReceiveMessage();
                }
                else
                {
                    ClientSocket.SendMessage(Constants.Client.LEAVE_ROOM);
                    ClientSocket.ReceiveMessage();
                }

                ClientSocket.Destructor();
                base.OnClosed(e);
                System.Environment.Exit(0);
            }
            ClientHelper.doClose = true;
        }
    }
}
