import { userActionTypes } from 'constants/ActionTypes';

import { authorityActions, crewActions } from 'store/actions';

import * as endpoint from './endpoints';

export const login = (username, password) => {
  return async dispatch => {
    dispatch({ type: userActionTypes.USER_LOGIN_REQUEST });
    try {
      const response = await endpoint.login(username, password);

      await dispatch(authorityActions.getAuthorities());
      await dispatch(crewActions.getCrews());

      dispatch({
        type: userActionTypes.USER_LOGIN_SUCCESS,
        payload: response.data
      });
    } catch (e) {
      dispatch({ type: userActionTypes.USER_LOGIN_FAILURE, payload: e });
      throw e;
    }
  };
};
