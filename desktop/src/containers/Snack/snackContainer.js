import React, { Component } from 'react'

import classNames from 'classnames'
import { Snackbar, SnackbarContent,Icon,IconButton } from '@material-ui/core';
import {Close} from '@material-ui/icons'
import green from '@material-ui/core/colors/green';
import amber from '@material-ui/core/colors/amber';
import { withStyles } from '@material-ui/core/styles';

class SnackContainer extends Component {
  render() {
    console.log(this.props);
    const {classes,data} = this.props;
    return (
      <SnackbarContent
          className={classes.info}
          aria-describedby="client-snackbar"
          message={
            <div>{data[0] + data[1]}</div>             
          }
          action={[
            <IconButton
              key="close"
              aria-label="Close"
              color="inherit"
              className={classes.close}
              onClick={this.props.closeSnack}
            >
              <Close className={classes.icon} />
            </IconButton>,
          ]}
        />
    )
  }
}
  // {/* <span id="client-snackbar" className={classes.message}>
  //             <Icon className={classNames(classes.icon, classes.iconVariant)} /> */}
  //             <div>{data}</div> 
  //           {/* </span> */}

const styles1 = theme => ({
  success: {
    backgroundColor: green[600],
  },
  error: {
    backgroundColor: theme.palette.error.dark,
  },
  info: {
    backgroundColor: theme.palette.primary.dark,
  },
  warning: {
    backgroundColor: amber[700],
  },
  icon: {
    fontSize: 20,
  },
  iconVariant: {
    opacity: 0.9,
    marginRight: theme.spacing.unit,
  },
  message: {
    display: 'flex',
    alignItems: 'center',
  },
});

export default withStyles(styles1)(SnackContainer);