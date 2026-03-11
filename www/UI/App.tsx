import { Provider, useAtomValue, useSetAtom } from 'jotai';
import React, {
  ReactElement,
  SyntheticEvent,
  useCallback,
  useRef,
} from 'react';
import ReactDiffViewer, {
  LineNumberPrefix,
  ReactDiffViewerStyles,
} from 'react-diff-viewer-continued';
import { KeepAlive } from 'www/KeepAlive';
import { useJotaiAsyncCallback } from 'www/State/Helpers';
import { getStore } from 'www/State/Storage';
import { WebSocketRegistrar } from 'www/Tools/ReactWebSocket';
import { isValidRefObject } from 'www/Utils';

import {
  DiffInformation,
  DiffType,
} from 'node_modules/react-diff-viewer-continued/lib/cjs/src/compute-lines';
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
  /* 
        splitView={true}
        alwaysShowLines={['L1', 'R3']}
        hideLineNumbers={true}
        highlightLines={['R2', 'L4']}
        useDarkTheme={true}
        extraLinesSurroundingDiff={2}
        onLineNumberClick={(
          lnId: string,
          evt: React.MouseEvent<HTMLTableCellElement>,
        ) => {}}
        renderGutter={(diffData: {
          lineNumber: number;
          type: DiffType;
          prefix: LineNumberPrefix;
          value: string | DiffInformation[];
          additionalLineNumber: number;
          additionalPrefix: LineNumberPrefix;
          styles: ReactDiffViewerStyles;
        }) => <div>{diffData.lineNumber}</div>}
*/
  return (
    <div id="app">
      <ReactDiffViewer
        leftTitle={'Old Code'}
        rightTitle={'New Code'}
        loadingElement={() => <span>LOADING!!!</span>}
        oldValue={oldCode}
        newValue={newCode}
      />
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
