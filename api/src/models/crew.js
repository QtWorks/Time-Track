/* jshint indent: 1 */

module.exports = function(sequelize, DataTypes) {
  return sequelize.define(
    'crew',
    {
      id: {
        type: DataTypes.INTEGER(11),
        //allowNull: false,
        primaryKey: true,
      },
      name: {
        type: DataTypes.STRING(45),
        allowNull: false,
        unique: true,
      },
    },
    {
      tableName: 'crew',
      timestamps: false,
    },
  );
};
