import React from 'react';
import PropTypes from 'prop-types';

import { TextField as MUTextField } from '@material-ui/core';

const TextField = ({ field, form, id, label }) => (
  <MUTextField
    {...field}
    id={id}
    label={label}
    type="password"
    InputLabelProps={{ shrink: true }}
    fullWidth
    margin="normal"
    FormHelperTextProps={{ error: true }}
    helperText={
      form.errors[field.name] && form.touched[field.name]
        ? form.errors[field.name]
        : ' '
    }
  />
);

TextField.propTypes = {
  field: PropTypes.object.isRequired,
  form: PropTypes.object.isRequired,
  id: PropTypes.string,
  label: PropTypes.string.isRequired
};

export default TextField;
