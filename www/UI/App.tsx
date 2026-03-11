import { Provider, useAtomValue, useSetAtom } from 'jotai';
import { ReactElement, SyntheticEvent, useCallback, useRef } from 'react';
import ReactDiffViewer from 'react-diff-viewer-continued';
import { KeepAlive } from 'www/KeepAlive';
import { useJotaiAsyncCallback } from 'www/State/Helpers';
import { getStore } from 'www/State/Storage';
import { WebSocketRegistrar } from 'www/Tools/ReactWebSocket';
import { isValidRefObject } from 'www/Utils';

import '../styles/App.css';

function TheActualApp(): ReactElement {
  const oldCode = `
const a = 10
const b = 10
const c = () => console.log('foo')

if(a > 10) {
  console.log('bar')
}

console.log('done')
`;
  const newCode = `
const a = 10
const boo = 10

if(a === 10) {
  console.log('bar')
}
`; // https://github.com/Aeolun/react-diff-viewer-continued
  return (
    <div id="app">
      <ReactDiffViewer oldValue={oldCode} newValue={newCode} splitView={true} />
    </div>
  );
}

export function App(): ReactElement {
  const store = getStore();
  return (
    <Provider store={store}>
      <WebSocketRegistrar />
      <KeepAlive />
      <TheActualApp />
    </Provider>
  );
}
