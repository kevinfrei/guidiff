import {
  arr,
  bool,
  Enum,
  enum_lst,
  enum_num,
  enum_str,
  fmap,
  i16,
  i32,
  i8,
  map,
  NEnum,
  num,
  obj,
  ObjType,
  opt,
  ref,
  SEnum,
  str,
  sub,
  tup,
  Types,
  u16,
  u8,
} from './IDL';

const CurrentView: NEnum = enum_num(num(), {
  disabled: -1,
  none: 0,
  recent: 1,
  albums: 2,
  artists: 3,
  songs: 4,
  playlists: 5,
  now_playing: 6,
  settings: 7,
  search: 8,
  tools: 9,
});

const StrId: SEnum = enum_str({
  ChooseFile: 'Choose File...',
  ViewSettings: 'Settings',
});

const Keys: SEnum = enum_str({
  AddFileLocation: 'O',
  Albums: '2',
  Artists: '3',
  Backward10s: '[',
  Find: 'F',
  Forward10s: ']',
  NextTrack: 'Right',
  NowPlaying: '1',
  Play: 'P',
  Playlists: '5',
  PreviousTrack: 'Left',
  Repeat: 'T',
  SavePlaylist: 'S',
  Settings: ',',
  Shuffle: 'R',
  Songs: '4',
  ToggleMiniPlayer: '9',
  Tools: 'L',
});

const IpcCall: NEnum = enum_num(u8(), {
  Unknown: 0,
  ReadFromStorage: 1,
  WriteToStorage: 2,
  DeleteFromStorage: 3,
  AsyncData: 4,
  IsDev: 5,
  MenuAction: 6,
  MinimizeWindow: 7,
  MaximizeWindow: 8,
  RestoreWindow: 9,
  CloseWindow: 10,
  ShowOpenDialog: 11,
});

const SocketMsg: Enum = enum_lst(u8(), [
  'Unknown',
  'ContentLoaded',
  'KeepAlive',
]);

const StorageId: SEnum = enum_str({
  CurrentView: 'currentView',
});

const MimeData = obj({
  type: str(),
  data: str(),
});

const FileFilterItem = obj({ name: str(), extensions: arr(str()) });

const OpenDialogOptions = obj({
  folder: opt(bool()),
  title: opt(str()),
  defaultPath: opt(str()),
  buttonLabel: opt(str()),
  multiSelections: opt(bool()),
  filters: opt(arr(ref('FileFilterItem'))),
});

export const TypesToGenerate: Record<string, Types> = {
  Keys,
  StrId,
  CurrentView,
  IpcCall,
  SocketMsg,
  StorageId,
  MimeData,
  FileFilterItem,
  OpenDialogOptions,
};

export const PicklersToGenerate: Record<string, Types> = {};
