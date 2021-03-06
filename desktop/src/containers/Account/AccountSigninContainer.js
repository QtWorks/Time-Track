import React, { Component } from 'react';
import { connect } from 'react-redux';
import { withRouter } from 'react-router-dom';
import PropTypes from 'prop-types';

import { Formik } from 'formik';

import { account as accountValidation } from 'constants/formValidation';
import { employeeActions, staticActions } from 'store/actions';
import AccountSigin from 'components/forms/AccountSigin';

class SignInContainer extends Component {
  render() {
    const { login, history, getStaticData, authorityEntities } = this.props;
    return (
      <Formik
        initialValues={{ pin: '565656' }}
        validationSchema={accountValidation}
        onSubmit={values => {
          login(values.pin).then(asdf => {
            const { authorityId } = asdf.data;
            history.push(`/${authorityEntities[authorityId].type}`);
          });
          getStaticData();
        }}
        render={formProps => <AccountSigin {...formProps} />}
      />
    );
  }
}

SignInContainer.propTypes = {
  login: PropTypes.func.isRequired,
  history: PropTypes.object.isRequired
};

const mapStateToProps = state => {
  return {
    authorityEntities: state.entities.authorities
  };
};

const mapDispatchToProps = dispatch => {
  return {
    login: pin => {
      return dispatch(employeeActions.login(pin));
    },
    getStaticData: () => {
      return dispatch(staticActions.getStaticData());
    }
  };
};

export default withRouter(
  connect(
    mapStateToProps,
    mapDispatchToProps,
  )(SignInContainer),
);
