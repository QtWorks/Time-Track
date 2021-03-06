import accountActionScene from 'scenes/Account/accountActionScene';
import clockOutScene from 'scenes/Clock/clockOutScene';
import exportScene from 'scenes/Export/exportScene';
import * as routes from 'constants/routes';

export default [
  {
    path: `/${routes.MANAGER}`,
    component: accountActionScene,
    exact: true,
    name: 'Manager Actions',
    type: routes.MANAGER
  }, {
    path: `/${routes.MANAGER}/${routes.CLOCKOUT}`,
    component: clockOutScene,
    exact: true,
    name: 'Manager Clockout',
    type: routes.MANAGER
  }, {
    path: `/${routes.MANAGER}/${routes.EXPORT}`,
    component: exportScene,
    exact: true,
    name: 'Manager Export',
    type: routes.MANAGER
  }
];
